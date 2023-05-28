using Microsoft.AspNetCore.Mvc;
using SPM.Api.Services.Admin.Account;
using Microsoft.AspNetCore.Authorization;
using SPM.Api.Services.Admin.Account.Models;

namespace SPM.Api.Controllers
{
    [ApiController]
    [Route("Admin/Account/[action]")]
    [Authorize(Policy = "AdminUserPolicy")]
    public class AdminAccountController : ControllerBase
    {
        private readonly IAdminAccountService _accountService;

        public AdminAccountController(IAdminAccountService accountService)
        {
            _accountService = accountService;
        }

        [HttpGet]
        public async Task<IEnumerable<GetUsersListItem>> GetUsers()
        {
            return await _accountService.GetUsers();
        }
    }
}
