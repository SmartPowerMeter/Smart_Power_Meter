using Microsoft.AspNetCore.Mvc;
using SPM.Api.Core.Domain.Enums;
using SPM.Api.Services.InfluxDb.Models;
using Microsoft.AspNetCore.Authorization;
using SPM.Api.Services.Measurements.Models;
using SPM.Api.Services.Customer.Measurement;

namespace SPM.Api.Controllers
{
    [ApiController]
    [Route("Measurement/[action]")]
    public class CustomerMeasurementController : ControllerBase
    {
        private readonly ICustomerMeasurementService _measurementService;

        public CustomerMeasurementController(ICustomerMeasurementService measurementService)
        {
            _measurementService = measurementService;
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
        public async Task<GetEnergyConsumptionResponse> GetEnergyConsumption(TimeRange timeRange)
        {
            return await _measurementService.GetEnergyConsumption(timeRange);
        }
    }
}
