using Microsoft.AspNetCore.Mvc;
using SPM.Api.Services.Admin.Relay;
using Microsoft.AspNetCore.Authorization;

namespace SPM.Api.Controllers
{
    [ApiController]
    [Route("Admin/Relay/[action]")]
    [Authorize(Policy = "AdminUserPolicy")]
    public class AdminRelayController : ControllerBase
    {
        private readonly IAdminRelayService _relayService;

        public AdminRelayController(IAdminRelayService relayService)
        {
            _relayService = relayService;
        }

        [HttpPost("{customerId}")]
        public async Task<bool> SetRelayStatus(string customerId, bool activate)
        {
            return await _relayService.SetRelayStatus(customerId, activate);
        }

        [HttpGet("{customerId}")]
        public async Task<bool> GetRelayStatus(string customerId)
        {
            return await _relayService.GetRelayStatus(customerId);
        }
    }
}
