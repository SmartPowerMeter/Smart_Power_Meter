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
            return await _measurementService.GetMeasurement(request.MeasurementType, request.TimeRange, request.AggregateDuration);
        }

        [Authorize]
        [HttpPost]
        public async Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(GetRecentMeasurementRequest request)
        {
            return await _measurementService.GetRecentMeasurement(request.MeasurementType, request.TimeType, request.Time);
        }

        [Authorize]
        [HttpGet]
        public async Task<GetMonthlyEnergyConsumptionResponse> GetMonthlyEnergyConsumption()
        {
            return await _measurementService.GetMonthlyEnergyConsumption();
        }

        [Authorize]
        [HttpPost]
        public async Task<bool> SetRelayStatus(bool activate)
        {
            return await _mqttService.SetRelayStatus(activate);
        }

        [Authorize]
        [HttpGet]
        public async Task<bool> GetRelayStatus()
        {
            return await _mqttService.GetRelayStatus();
        }
    }
}
