using MQTTnet;
using System.Text;
using MQTTnet.Server;
using MQTTnet.Protocol;
using System.Text.Json;
using MQTT.Broker.Models;
using MQTT.Broker.Services.SqlDb;
using MQTT.Broker.Services.InfluxDb;
using MQTT.Broker.Services.SqlDb.Models;

namespace MQTT.Broker
{
    public class Worker : BackgroundService
    {
        private readonly ILogger<Worker> _logger;
        private readonly IServiceProvider _serviceProvider;
        private MqttServer _mqttServer;
        private UserData _user;

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
            _mqttServer.ClientDisconnectedAsync += ClientDisconnectedAsync;

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
            if (arg.ClientId != "admin" && arg.ClientId != "relay")
            {
                using var scope = _serviceProvider.CreateScope();
                var dbService = scope.ServiceProvider.GetRequiredService<IDbService>();

                _user = await dbService.GetUser(arg.ClientId);

                if (_user == null)
                    arg.ReasonCode = MqttConnectReasonCode.BadUserNameOrPassword;
            }

            await Task.CompletedTask;
        }

        private async Task ClientConnectedAsync(ClientConnectedEventArgs arg)
        {
            await Task.Run(() => _logger.LogInformation($"{DateTimeOffset.Now} - New connection: ClientId = {arg.ClientId}, Endpoint = {arg.Endpoint}"));
        }

        private async Task MessageReceivedAsync(InterceptingPublishEventArgs arg)
        {
            if (arg.ApplicationMessage.Topic.Contains("measurement"))
            {
                var payload = arg.ApplicationMessage?.Payload == null ? null : Encoding.UTF8.GetString(arg.ApplicationMessage?.Payload);

                try
                {
                    using var scope = _serviceProvider.CreateScope();
                    var dbService = scope.ServiceProvider.GetRequiredService<IDbService>();

                    _user = await dbService.GetUser(arg.ClientId);

                    var data = JsonSerializer.Deserialize<MeasurementData>(payload);

                    var influxDbService = scope.ServiceProvider.GetRequiredService<InfluxDbService>();

                    influxDbService.WritePoint(nameof(data.Voltage), data.Voltage, data.TimeStamp, _user);
                    influxDbService.WritePoint(nameof(data.Current), data.Current, data.TimeStamp, _user);
                    influxDbService.WritePoint(nameof(data.Power), data.Power, data.TimeStamp, _user);
                    influxDbService.WritePoint(nameof(data.Energy), data.Energy, data.TimeStamp, _user);
                    influxDbService.WritePoint(nameof(data.Frequency), data.Frequency, data.TimeStamp, _user);
                    influxDbService.WritePoint(nameof(data.PowerFactor), data.PowerFactor, data.TimeStamp, _user);
                }
                catch (Exception ex)
                {
                    await Task.Run(() => _logger.LogError(ex.ToString()));
                }
            }
        }

        private async Task ClientDisconnectedAsync(ClientDisconnectedEventArgs arg)
        {
            await Task.Run(() => _logger.LogInformation($"{DateTimeOffset.Now} - New connection: ClientId = {arg.ClientId}, Endpoint = {arg.Endpoint}, DisconnectType = {arg.DisconnectType}"));
        }
    }
}