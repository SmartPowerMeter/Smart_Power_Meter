using MQTT.Broker.Services.SqlDb.Models;

namespace MQTT.Broker.Services.SqlDb
{
    public interface IDbService
    {
        Task<UserData> GetUser(string customerId);
    }
}
