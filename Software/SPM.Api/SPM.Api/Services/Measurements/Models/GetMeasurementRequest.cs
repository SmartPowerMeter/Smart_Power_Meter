using SPM.Api.Core.Domain.Enums;

namespace SPM.Api.Services.Measurements.Models
{
    public class GetMeasurementRequest
    {
        public MeasurementType MeasurementType { get; set; }

        public TimeRange TimeRange { get; set; }

        public AggregateDuration AggregateDuration { get; set; }
    }
}
