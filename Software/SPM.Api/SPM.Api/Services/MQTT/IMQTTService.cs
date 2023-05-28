namespace SPM.Api.Services.MQTT
{
    public interface IMQTTService
    {
        Task<bool> SetRelayStatus(string customerId, bool activate, bool isAdminCommand);

        Task<bool> GetRelayStatus(string customerId);
    }
}
