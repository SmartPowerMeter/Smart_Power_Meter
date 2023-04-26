using MQTTnet;
using System.Text;
using SPM.Api.Data;
using MQTTnet.Client;
using SPM.Api.Core.WorkContexts;
using Microsoft.EntityFrameworkCore;

namespace SPM.Api.Services.MQTT
{
    public class MQTTService : IMQTTService
    {
        private readonly IWorkContext _workContext;
        private readonly SPMDbContext _dbContext;
        private readonly string _brokerUrl;

        public MQTTService(IWorkContext workContext, SPMDbContext dbContext, IConfiguration configuration)
        {
            _workContext = workContext;
            _dbContext = dbContext;
            _brokerUrl = configuration.GetValue<string>("BrokerUrl");
        }

        public async Task<bool> SetRelayStatus(bool activate)
        {
            var user = await _dbContext.User.FirstAsync(x => x.Id == _workContext.CurrentUser.Id);

            var options = new MqttClientOptionsBuilder()
                .WithClientId("admin")
                .WithTcpServer(_brokerUrl, 707)
                .WithCredentials("user", "password")
                .Build();

            var mqttClient = new MqttFactory().CreateMqttClient();

            mqttClient.ConnectedAsync += async e =>
            {
                await mqttClient.SubscribeAsync($"relay-state-{user.CustomerId}");
            };

            mqttClient.ApplicationMessageReceivedAsync += async e =>
            {
                var payload = Encoding.UTF8.GetString(e.ApplicationMessage.Payload);

                user.SetRelayState(payload == "0");
                await _dbContext.SaveChangesAsync();
            };

            try
            {
                await mqttClient.ConnectAsync(options);
            }
            catch
            {
                await mqttClient.ConnectAsync(options);
            }

            var message = new MqttApplicationMessageBuilder()
                .WithTopic($"relay-{user.CustomerId}")
                .WithPayload($"{(activate ? 1 : 0)}")
                .Build();

            await mqttClient.PublishAsync(message);

            await Task.Delay(1500);

            return user.RelayEnabled;
        }
    }
}
