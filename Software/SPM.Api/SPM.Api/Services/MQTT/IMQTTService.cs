namespace SPM.Api.Services.MQTT
{
    public interface IMQTTService
    {
        Task<bool> SetRelayStatus(bool activate);

        Task<bool> GetRelayStatus();
    }
}
