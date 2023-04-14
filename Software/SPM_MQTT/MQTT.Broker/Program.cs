using Serilog;
using MQTT.Broker;
using MQTT.Broker.Services.InfluxDb;

var host = Host.CreateDefaultBuilder(args)
    .ConfigureServices(services =>
    {
        services.AddScoped<InfluxDbService>();
        services.AddHostedService<Worker>();
    })
    .Build();

Log.Logger = new LoggerConfiguration().MinimumLevel.Information().Enrich.FromLogContext().WriteTo.Console().CreateLogger();

await host.RunAsync();
