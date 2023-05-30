using Microsoft.AspNetCore.Mvc;
using SPM.Api.Services.MQTT.Models;
using SPM.Api.Services.Customer.Relay;
using Microsoft.AspNetCore.Authorization;

namespace SPM.Api.Controllers
{
    [ApiController]
    [Route("Relay/[action]")]
    public class CustomerRelayController : ControllerBase
    {
        private readonly ICustomerRelayService _relayService;

        public CustomerRelayController(ICustomerRelayService relayService)
        {
            _relayService = relayService;
        }

        [Authorize]
        [HttpPost]
        public async Task<bool> SetRelayStatus(bool activate)
        {
            return await _relayService.SetRelayStatus(activate);
        }

        [Authorize]
        [HttpGet]
        public async Task<GetRelayStatusResponse> GetRelayStatus()
        {
            return await _relayService.GetRelayStatus();
        }
    }
}
