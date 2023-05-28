namespace SPM.Api.Services.Customer.Relay
{
    public interface ICustomerRelayService
    {
        Task<bool> SetRelayStatus(bool activate);

        Task<bool> GetRelayStatus();
    }
}
