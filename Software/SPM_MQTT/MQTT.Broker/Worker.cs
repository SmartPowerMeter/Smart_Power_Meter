using MQTTnet;
using System.Text;
using MQTTnet.Server;
using MQTTnet.Protocol;
using System.Text.Json;
using MQTT.Broker.Models;
using MQTT.Broker.Services.InfluxDb;

namespace MQTT.Broker
{
    public class Worker : BackgroundService
    {
        private readonly ILogger<Worker> _logger;
        private readonly IServiceProvider _serviceProvider;
        private MqttServer _mqttServer;

        public Worker(IServiceProvider serviceProvider, ILogger<Worker> logger)
        {
            _serviceProvider = serviceProvider;
            _logger = logger;
        }

        public async override Task StartAsync(CancellationToken cancellationToken)
        {
            var options = new MqttServerOptionsBuilder()
                .WithDefaultEndpoint()
                .WithDefaultEndpointPort(707)
                .Build();

            _mqttServer = new MqttFactory().CreateMqttServer(options);
            _mqttServer.ClientConnectedAsync += ClientConnectedAsync;
            _mqttServer.InterceptingPublishAsync += MessageReceivedAsync;
            _mqttServer.ValidatingConnectionAsync += ValidatingConnectionAsync;

            await _mqttServer.StartAsync();
            await base.StartAsync(cancellationToken);
        }

        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            while (!stoppingToken.IsCancellationRequested)
            {
            }
        }

        public override async Task StopAsync(CancellationToken cancellationToken)
        {
            await _mqttServer.StopAsync();
            await base.StopAsync(cancellationToken);
        }

        private async Task ValidatingConnectionAsync(ValidatingConnectionEventArgs arg)
        {
            // imitatingDbCall
            // var retrievedClientId = "12345";
            // var retrievedPassword = "pass";

            // if (arg.ClientId != retrievedClientId || arg.Password != retrievedPassword)
            // {
            //     arg.ReasonCode = MqttConnectReasonCode.BadUserNameOrPassword;
            // }

            await Task.CompletedTask;
        }

        private async Task ClientConnectedAsync(ClientConnectedEventArgs arg)
        {
            await Task.Run(() => _logger.LogInformation($"New connection: ClientId = {arg.ClientId}, Endpoint = {arg.Endpoint}, UserName = {arg.UserName}"));
        }

        private async Task MessageReceivedAsync(InterceptingPublishEventArgs arg)
        {
            var payload = arg.ApplicationMessage?.Payload == null ? null : Encoding.UTF8.GetString(arg.ApplicationMessage?.Payload);

            try
            {
                var data = JsonSerializer.Deserialize<MeasurementData>(payload);

                using var scope = _serviceProvider.CreateScope();
                var influxDbService = scope.ServiceProvider.GetRequiredService<InfluxDbService>();

                influxDbService.WritePoint(nameof(data.Voltage), arg.ClientId, data.Voltage);
                influxDbService.WritePoint(nameof(data.Current), arg.ClientId, data.Current);
                influxDbService.WritePoint(nameof(data.Power), arg.ClientId, data.Power);
                influxDbService.WritePoint(nameof(data.Energy), arg.ClientId, data.Energy);
                influxDbService.WritePoint(nameof(data.Frequency), arg.ClientId, data.Frequency);
                influxDbService.WritePoint(nameof(data.PowerFactor), arg.ClientId, data.PowerFactor);
            }
            catch (Exception ex)
            {
                await Task.Run(() => _logger.LogError(ex.ToString()));
            }
        }
    }
}