using Microsoft.AspNetCore.Mvc;
using SPM.Api.Core.Domain.Enums;
using SPM.Api.Services.InfluxDb.Models;
using Microsoft.AspNetCore.Authorization;
using SPM.Api.Services.Admin.Measurement;
using SPM.Api.Services.Measurements.Models;

namespace SPM.Api.Controllers
{
    [ApiController]
    [Route("Admin/Measurement/[action]")]
    [Authorize(Policy = "AdminUserPolicy")]
    public class AdminMeasurementController : ControllerBase
    {
        private readonly IAdminMeasurementService _measurementService;

        public AdminMeasurementController(IAdminMeasurementService measurementService)
        {
            _measurementService = measurementService;
        }

        [HttpPost("{customerId}")]
        public async Task<IEnumerable<MeasurementModel>> GetMeasurement(string customerId, GetMeasurementRequest request)
        {
            return await _measurementService.GetMeasurement(customerId, request.MeasurementType, request.TimeRange, request.AggregateDuration);
        }

        [HttpPost("{customerId}")]
        public async Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(string customerId, GetRecentMeasurementRequest request)
        {
            return await _measurementService.GetRecentMeasurement(customerId, request.MeasurementType, request.TimeType, request.Time);
        }

        [HttpGet("{customerId}")]
        public async Task<GetEnergyConsumptionResponse> GetEnergyConsumption(string customerId, TimeRange timeRange)
        {
            return await _measurementService.GetEnergyConsumption(customerId, timeRange);
        }
    }
}
