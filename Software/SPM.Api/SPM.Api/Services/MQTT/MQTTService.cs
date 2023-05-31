using MQTTnet;
using System.Text;
using SPM.Api.Data;
using MQTTnet.Client;
using SPM.Api.Services.MQTT.Models;
using Microsoft.EntityFrameworkCore;

namespace SPM.Api.Services.MQTT
{
    public class MQTTService : IMQTTService
    {
        private readonly SPMDbContext _dbContext;
        private readonly string _brokerUrl;

        public MQTTService(SPMDbContext dbContext, IConfiguration configuration)
        {
            _dbContext = dbContext;
            _brokerUrl = configuration.GetValue<string>("BrokerUrl");
        }

        public async Task<bool> SetRelayStatus(string customerId, bool activate, bool isAdminCommand)
        {
            var user = await _dbContext.User.FirstAsync(x => x.CustomerId == customerId);

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

                bool isEnabled = payload == "0";

                if (isAdminCommand)
                    user.SetAdminRelayState(isEnabled);
                else
                {
                    if (user.AdminRelayEnabled)
                        user.SetCustomerRelayState(isEnabled);
                }

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

            return isAdminCommand ? user.AdminRelayEnabled : user.CustomerRelayEnabled;
        }

        public async Task<GetRelayStatusResponse> GetRelayStatus(string customerId)
        {
            var user = await _dbContext.User.FirstAsync(x => x.CustomerId == customerId);

            return new GetRelayStatusResponse
            {
                CustomerRelayEnabled = user.CustomerRelayEnabled,
                AdminRelayEnabled = user.AdminRelayEnabled
            };
        }
    }
}
