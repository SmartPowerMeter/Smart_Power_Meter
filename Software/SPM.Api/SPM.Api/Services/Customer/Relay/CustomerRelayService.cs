using SPM.Api.Data;
using SPM.Api.Services.MQTT;
using SPM.Api.Core.WorkContexts;
using SPM.Api.Services.MQTT.Models;

namespace SPM.Api.Services.Customer.Relay
{
    public class CustomerRelayService : ICustomerRelayService
    {
        private readonly IMQTTService _mqttService;
        private readonly SPMDbContext _dbContext;
        private readonly string _customerId;

        public CustomerRelayService(IMQTTService mqttService, IWorkContext workContext, SPMDbContext dbContext)
        {
            _mqttService = mqttService;
            _dbContext = dbContext;
            _customerId = dbContext.User.First(x => x.Id == workContext.CurrentUser.Id).CustomerId;
        }

        public async Task<bool> SetRelayStatus(bool activate)
        {
            var user = _dbContext.User.First(x => x.CustomerId == _customerId);

            if (user.AdminRelayEnabled)
                return await _mqttService.SetRelayStatus(_customerId, activate, false);

            throw new InvalidOperationException("The relay is turned off globally");
        }

        public async Task<GetRelayStatusResponse> GetRelayStatus()
        {
            return await _mqttService.GetRelayStatus(_customerId);
        }
    }
}
