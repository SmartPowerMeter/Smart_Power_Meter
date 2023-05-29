namespace SPM.Api.Services.MQTT.Models
{
    public class GetRelayStatusResponse
    {
        public bool CustomerRelayEnabled { get; set; }

        public bool AdminRelayEnabled { get; set; }
    }
}
