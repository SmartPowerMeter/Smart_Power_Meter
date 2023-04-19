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

        public MeasurementController(IMeasurementService measurementService)
        {
            _measurementService = measurementService;
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
    }
}
