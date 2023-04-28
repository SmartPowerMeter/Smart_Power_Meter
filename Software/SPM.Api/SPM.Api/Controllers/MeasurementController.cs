using SPM.Api.Services.MQTT;
using Microsoft.AspNetCore.Mvc;
using SPM.Api.Services.Measurements;
using SPM.Api.Services.InfluxDb.Models;
using Microsoft.AspNetCore.Authorization;
using SPM.Api.Services.Measurements.Models;

namespace SPM.Api.Controllers
{
    [ApiController]
    [Route("[controller]/[action]")]
    public class MeasurementController : ControllerBase
    {
        private readonly IMeasurementService _measurementService;
        private readonly IMQTTService _mqttService;

        public MeasurementController(IMeasurementService measurementService, IMQTTService mqttService)
        {
            _measurementService = measurementService;
            _mqttService = mqttService;
        }

        [Authorize]
        [HttpPost]
        public async Task<IEnumerable<MeasurementModel>> GetMeasurement(GetMeasurementRequest request)
        {
            var data = await _measurementService.GetMeasurement(request.MeasurementType, request.TimeRange, request.AggregateDuration);

            return data;
        }

        [Authorize]
        [HttpPost]
        public async Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(GetRecentMeasurementRequest request)
        {
            var data = await _measurementService.GetRecentMeasurement(request.MeasurementType, request.TimeType, request.Time);

            return data;
        }

        [Authorize]
        [HttpPost]
        public async Task<bool> SetRelayStatus(bool activate)
        {
            var isActivated = await _mqttService.SetRelayStatus(activate);

            return isActivated;
        }

        [Authorize]
        [HttpGet]
        public async Task<bool> GetRelayStatus()
        {
            var isActivated = await _mqttService.GetRelayStatus();

            return isActivated;
        }
    }
}
