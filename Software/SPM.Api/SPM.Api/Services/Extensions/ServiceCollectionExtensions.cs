using System.Text;
using SPM.Api.Services.Jwt;
using SPM.Api.Services.MQTT;
using SPM.Api.Core.Constants;
using System.Security.Claims;
using SPM.Api.Services.Account;
using Microsoft.OpenApi.Models;
using SPM.Api.Core.WorkContexts;
using SPM.Api.Services.InfluxDb;
using SPM.Api.Services.Measurements;
using Microsoft.IdentityModel.Tokens;
using SPM.Api.Core.WorkContexts.Models;
using Microsoft.AspNetCore.Authentication.JwtBearer;

namespace SPM.Api.Services.Extensions
{
    public static class ServiceCollectionExtensions
    {
        public static IServiceCollection AddServices(this IServiceCollection services, IConfiguration configuration)
        {
            services.AddScoped<IInfluxDbService, InfluxDbService>();
            services.AddScoped<IMeasurementService, MeasurementService>();
            services.AddScoped<IAccountService, AccountService>();
            services.AddScoped<IJwtService, JwtService>();
            services.AddScoped<IMQTTService, MQTTService>();

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
                {
                    var user = new LoggedInUser
                    {
                        Id = int.Parse(identity.FindFirst(ClaimsConstants.Id).Value),
                        Email = identity.FindFirst(ClaimTypes.Email).Value
                    };

                    workContext.SetUser(user);
                }

                return workContext;
            });

            return services;
        }
    }
}
