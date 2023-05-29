using SPM.Api.Services.MQTT.Models;

namespace SPM.Api.Services.Admin.Relay
{
    public interface IAdminRelayService
    {
        Task<bool> SetRelayStatus(string customerId, bool activate);

        Task<GetRelayStatusResponse> GetRelayStatus(string customerId);
    }
}
