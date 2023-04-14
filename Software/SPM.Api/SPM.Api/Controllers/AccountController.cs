using Microsoft.AspNetCore.Mvc;
using SPM.Api.Services.Account;
using SPM.Api.Services.Account.Models;
using Microsoft.AspNetCore.Authorization;

namespace SPM.Api.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class AccountController : ControllerBase
    {
        private readonly IAccountService _accountService;

        public AccountController(IAccountService accountService)
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
    }
}