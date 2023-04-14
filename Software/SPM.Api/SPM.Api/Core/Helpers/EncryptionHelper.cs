using System.Text;
using System.Security.Cryptography;

namespace SPM.Api.Core.Helpers
{
    public static class EncryptionHelper
    {
        private const string salt = "23DAC12CAA609A1EADFE5BC567D55949B7C864403C8ABD750F3448D3EB6AF873FA4FB48640A5E6BE104B1329E36B4C5066E3D82432798F0400A5DE632A6CB947";

        public static string HashPasword(string password)
        {
            var hash = Rfc2898DeriveBytes.Pbkdf2(Encoding.UTF8.GetBytes(password),
                Convert.FromHexString(salt), 200000, HashAlgorithmName.SHA512, 32);

            return Convert.ToHexString(hash);
        }
    }
}
