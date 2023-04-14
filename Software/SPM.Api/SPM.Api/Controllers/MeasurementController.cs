using Microsoft.AspNetCore.Mvc;
using SPM.Api.Services.Measurements;
using SPM.Api.Services.InfluxDb.Models;
using Microsoft.AspNetCore.Authorization;
using SPM.Api.Services.Measurements.Models;

namespace SPM.Api.Controllers
{
    [ApiController]
    [Route("[controller]")]
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
    }
}
