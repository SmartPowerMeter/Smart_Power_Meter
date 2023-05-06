using Serilog;
using MQTT.Broker;
using MQTT.Broker.Services.SqlDb;
using MQTT.Broker.Services.InfluxDb;

var host = Host.CreateDefaultBuilder(args)
    .ConfigureServices(services =>
    {
        services.AddScoped<InfluxDbService>();
        services.AddScoped<IDbService, DbService>();
        services.AddHostedService<Worker>();
    })
    .Build();

Log.Logger = new LoggerConfiguration()
    .ReadFrom.Configuration(new ConfigurationBuilder()
        .AddJsonFile("appsettings.json")
        .Build())
    .Enrich.FromLogContext()
    .CreateLogger();

Log.Logger.Information("Broker started");

await host.RunAsync();
