using System.Text;
using SPM.Api.Services.Jwt;
using SPM.Api.Services.MQTT;
using System.Security.Claims;
using SPM.Api.Services.Email;
using SPM.Api.Core.Extensions;
using Microsoft.OpenApi.Models;
using SPM.Api.Core.WorkContexts;
using SPM.Api.Services.InfluxDb;
using SPM.Api.Services.Admin.Relay;
using SPM.Api.Services.Measurements;
using SPM.Api.Services.Admin.Account;
using Microsoft.IdentityModel.Tokens;
using SPM.Api.Services.Customer.Relay;
using SPM.Api.Services.Customer.Account;
using SPM.Api.Services.Admin.Measurement;
using SPM.Api.Services.Customer.Measurement;
using Microsoft.AspNetCore.Authentication.JwtBearer;

namespace SPM.Api.Services.Extensions
{
    public static class ServiceCollectionExtensions
    {
        public static IServiceCollection AddServices(this IServiceCollection services, IConfiguration configuration)
        {
            services.AddScoped<IInfluxDbService, InfluxDbService>();
            services.AddScoped<IMeasurementService, MeasurementService>();
            services.AddScoped<IJwtService, JwtService>();
            services.AddScoped<IMQTTService, MQTTService>();
            services.AddScoped<IEmailService, EmailService>();

            services.AddScoped<ICustomerRelayService, CustomerRelayService>();
            services.AddScoped<ICustomerAccountService, CustomerAccountService>();
            services.AddScoped<ICustomerMeasurementService, CustomerMeasurementService>();

            services.AddScoped<IAdminRelayService, AdminRelayService>();
            services.AddScoped<IAdminAccountService, AdminAccountService>();
            services.AddScoped<IAdminMeasurementService, AdminMeasurementService>();

            services.AddJwt(configuration);
            services.AddSwagger();
            services.AddWorkContext();

            return services;
        }

        public static IServiceCollection AddJwt(this IServiceCollection services, IConfiguration configuration)
        {
            services.AddAuthentication(JwtBearerDefaults.AuthenticationScheme)
                .AddJwtBearer(options =>
                {
                    options.TokenValidationParameters = new TokenValidationParameters
                    {
                        ValidateIssuer = true,
                        ValidateAudience = true,
                        ValidateLifetime = true,
                        ValidateIssuerSigningKey = true,
                        ValidIssuer = configuration["Jwt:Issuer"],
                        ValidAudience = configuration["Jwt:Issuer"],
                        IssuerSigningKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(configuration["Jwt:Key"]))
                    };
                });

            return services;
        }

        public static IServiceCollection AddSwagger(this IServiceCollection services)
        {
            services.AddSwaggerGen(c =>
            {
                c.AddSecurityDefinition("Bearer", new OpenApiSecurityScheme
                {
                    In = ParameterLocation.Header,
                    Description = "Please enter a valid token",
                    Name = "Authorization",
                    Type = SecuritySchemeType.Http,
                    BearerFormat = "JWT",
                    Scheme = "Bearer"
                });
                c.AddSecurityRequirement(new OpenApiSecurityRequirement
                {
                    {
                        new OpenApiSecurityScheme
                        {
                            Reference = new OpenApiReference
                            {
                                Type = ReferenceType.SecurityScheme,
                                Id = "Bearer"
                            }
                        },
                        new string[]{}
                    }
                });
            });

            return services;
        }

        public static IServiceCollection AddWorkContext(this IServiceCollection services)
        {
            services.AddScoped<IWorkContext>((ctx) =>
            {
                var workContext = new WorkContext();

                var httpContext = ctx.GetService<IHttpContextAccessor>()?.HttpContext;

                if (httpContext == null)
                    return workContext;

                if (httpContext.User.Identity is ClaimsIdentity identity && identity.IsAuthenticated)
                    workContext.SetUser(httpContext.ToLoggedInUser());

                return workContext;
            });

            return services;
        }
    }
}
