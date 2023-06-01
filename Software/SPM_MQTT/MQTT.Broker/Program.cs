using Serilog;
using MQTT.Broker;
using MQTT.Broker.Services.SqlDb;
using MQTT.Broker.Services.InfluxDb;

Log.Logger = new LoggerConfiguration()
    .ReadFrom.Configuration(new ConfigurationBuilder()
        .AddJsonFile("appsettings.json")
        .Build())
    .Enrich.WithProperty("TimeZone", TimeZoneInfo.FromSerializedString("Asia/Tbilisi"))
    .CreateLogger();

var host = Host.CreateDefaultBuilder(args)
    .ConfigureServices(services =>
    {
        services.AddScoped<InfluxDbService>();
        services.AddScoped<IDbService, DbService>();
        services.AddHostedService<Worker>();
        services.AddLogging(x => x.AddSerilog(Log.Logger));
    })
    .Build();

Log.Logger.Information("Broker started");

await host.RunAsync();
