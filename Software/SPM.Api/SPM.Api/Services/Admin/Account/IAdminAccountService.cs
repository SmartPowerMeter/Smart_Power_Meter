using SPM.Api.Services.Admin.Account.Models;

namespace SPM.Api.Services.Admin.Account
{
    public interface IAdminAccountService
    {
        Task<IEnumerable<GetUsersListItem>> GetUsers();
    }
}
