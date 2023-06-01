using Serilog;
using SPM.Api.Data;
using SPM.Api.Policies;
using SPM.Api.Data.Extensions;
using SPM.Api.Services.Extensions;
using Microsoft.EntityFrameworkCore;
using Microsoft.AspNetCore.Authorization;

var builder = WebApplication.CreateBuilder(args);

var connectionString = builder.Configuration.GetConnectionString("DefaultConnection");
builder.Services.AddDbContext<SPMDbContext>(options =>
    options.UseSqlServer(connectionString));

builder.Services.AddControllers();

builder.Services.AddEndpointsApiExplorer();
builder.Services.AddServices(builder.Configuration);
builder.Services.AddHttpContextAccessor();

builder.Services.AddCors();

builder.Services.AddSingleton<IAuthorizationHandler, AdminUserHandler>();
builder.Services.AddAuthorization(options =>
{
    options.AddPolicy("AdminUserPolicy", policy =>
    {
        policy.Requirements.Add(new AdminUserRequirement());
    });
});

Log.Logger = new LoggerConfiguration()
    .ReadFrom.Configuration(new ConfigurationBuilder()
        .AddJsonFile("appsettings.json")
        .Build())
    .Enrich.FromLogContext()
    .CreateLogger();

builder.Services.AddLogging(x => x.AddSerilog(Log.Logger));

var app = builder.Build();

app.UseSwagger();
app.UseSwaggerUI();

app.Services.MigrateDb();

app.UseHttpsRedirection();

app.UseCors(x => x.AllowAnyOrigin().AllowAnyMethod().AllowAnyHeader());

app.UseAuthentication();
app.UseAuthorization();

app.MapControllers();

Log.Logger.Information("SPM_Api started");

app.Run();
