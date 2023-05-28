namespace SPM.Api.Services.Admin.Relay
{
    public interface IAdminRelayService
    {
        Task<bool> SetRelayStatus(string customerId, bool activate);

        Task<bool> GetRelayStatus(string customerId);
    }
}
