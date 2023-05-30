using Microsoft.AspNetCore.Mvc;
using SPM.Api.Services.Customer.Account;
using Microsoft.AspNetCore.Authorization;
using SPM.Api.Services.Customer.Account.Models;

namespace SPM.Api.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class AccountController : ControllerBase
    {
        private readonly ICustomerAccountService _accountService;

        public AccountController(ICustomerAccountService accountService)
        {
            _accountService = accountService;
        }

        [HttpPost("register")]
        public async Task RegisterUser(RegisterUserRequest request)
        {
            await _accountService.RegisterUser(request);
        }

        [HttpPost("login")]
        public async Task<LoginUserResponse> LoginUser(LoginUserRequest request)
        {
            return await _accountService.LoginUser(request);
        }

        [Authorize]
        [HttpGet("[action]")]
        public async Task<GetUserDataResponse> GetUserData()
        {
            return await _accountService.GetUserData();
        }

        [Authorize]
        [HttpPost("[action]")]
        public async Task ChangePassword(string newPassword)
        {
            await _accountService.ChangePassword(newPassword);
        }

        [HttpPost("[action]")]
        public async Task RecoverPassword(string email)
        {
            await _accountService.RecoverPassword(email);
        }
    }
}