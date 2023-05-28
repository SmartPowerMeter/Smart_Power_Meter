using SPM.Api.Services.MQTT;

namespace SPM.Api.Services.Admin.Relay
{
    public class AdminRelayService : IAdminRelayService
    {
        private readonly IMQTTService _mqttService;

        public AdminRelayService(IMQTTService mqttService)
        {
            _mqttService = mqttService;
        }

        public async Task<bool> SetRelayStatus(string customerId, bool activate)
        {
            return await _mqttService.SetRelayStatus(customerId, activate, true);
        }

        public async Task<bool> GetRelayStatus(string customerId)
        {
            return await _mqttService.GetRelayStatus(customerId);
        }
    }
}
