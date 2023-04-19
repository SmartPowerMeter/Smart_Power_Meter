using SPM.Api.Core.Domain.Enums;

namespace SPM.Api.Services.Measurements.Models
{
    public class GetRecentMeasurementRequest
    {
        public MeasurementType MeasurementType { get; set; }

        public TimeType TimeType { get; set; }

        public int Time { get; set; }
    }
}
