using SPM.Api.Core.Helpers;
using SPM.Api.Core.Domain.Enums;
using SPM.Api.Core.Domain.Models;

namespace SPM.Api.Data
{
    public static class DbInitializer
    {
        public static void SeedData(IServiceProvider serviceProvider)
        {
            using var serviceScope = serviceProvider.GetRequiredService<IServiceScopeFactory>().CreateScope();
            using var context = serviceScope.ServiceProvider.GetService<SPMDbContext>();

            var adminUsers = GetAdminUsers();

            foreach (var adminUser in adminUsers)
            {
                var dbAdminUser = context.User.FirstOrDefault(x => x.Email == adminUser.Email);

                if (dbAdminUser == null)
                {
                    context.User.Add(adminUser);
                }
            }

            context.SaveChanges();
        }

        private static List<User> GetAdminUsers()
        {
            var adminUser = new User("admin", "admin", "smartpowmeter@gmail.com", EncryptionHelper.HashPasword("smart2meter5pass"), UserType.Admin);
            adminUser.SetBucketAccessToken("token");

            var adminUsers = new List<User>
            {
                adminUser
            };

            return adminUsers;
        }
    }
}