using SPM.Api.Data;
using SPM.Api.Core.Domain.Enums;
using Microsoft.EntityFrameworkCore;
using SPM.Api.Services.Admin.Account.Models;

namespace SPM.Api.Services.Admin.Account
{
    public class AdminAccountService : IAdminAccountService
    {
        private readonly SPMDbContext _dbContext;

        public AdminAccountService(SPMDbContext dbContext)
        {
            _dbContext = dbContext;
        }

        public async Task<IEnumerable<GetUsersListItem>> GetUsers()
        {
            var users = await _dbContext.User.Where(x => x.UserType == UserType.Connected).ToListAsync();

            return users.Select(x => new GetUsersListItem
            {
                CustomerId = x.CustomerId,
                Email = x.Email,
                FirstName = x.FirstName,
                LastName = x.LastName,
                CustomerRelayEnabled = x.CustomerRelayEnabled,
                AdminRelayEnabled = x.AdminRelayEnabled
            });
        }
    }
}
