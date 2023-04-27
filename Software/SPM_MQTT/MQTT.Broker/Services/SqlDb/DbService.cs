using System.Data.SqlClient;
using MQTT.Broker.Services.SqlDb.Models;

namespace MQTT.Broker.Services.SqlDb
{
    public class DbService : IDbService
    {
        private readonly string _connectionString;

        public DbService(IConfiguration configuration)
        {
            _connectionString = configuration.GetConnectionString("DefaultConnection");
        }

        public async Task<UserData> GetUser(string customerId)
        {
            using var connection = new SqlConnection(_connectionString);
            await connection.OpenAsync();

            var command = new SqlCommand("SELECT CustomerId, BucketAccessToken FROM [SmartPowerMeter].[dbo].[User] WHERE CustomerId = @CustomerId", connection);
            command.Parameters.AddWithValue("@CustomerId", customerId);

            var reader = await command.ExecuteReaderAsync();

            try
            {
                if (await reader.ReadAsync())
                {
                    var user = new UserData
                    {
                        CustomerId = reader.GetString(0),
                        BucketAccessToken = reader.GetString(1)
                    };

                    await reader.CloseAsync();

                    return user;
                }
            }
            catch
            {
            }
            finally
            {
                await connection.CloseAsync();
            }

            return null;
        }
    }
}
