namespace MQTT.Broker.Models
{
    public class MeasurementData
    {
        public double Voltage { get; set; }

        public double Current { get; set; }

        public double Power { get; set; }

        public double Energy { get; set; }

        public double Frequency { get; set; }

        public double PowerFactor { get; set; }

        public DateTime TimeStamp { get; set; }
    }
}
