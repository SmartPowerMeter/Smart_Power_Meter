using System.Net;
using System.Net.Mail;

namespace SPM.Api.Services.Email
{
    public class EmailService : IEmailService
    {
        private readonly string _senderEmail;
        private readonly string _senderPassword;

        public EmailService(IConfiguration configuration)
        {
            _senderEmail = configuration.GetValue<string>("Email:SenderEmail");
            _senderPassword = configuration.GetValue<string>("Email:SenderPassword");
        }

        public async Task SendRecoveryEmail(string recipientEmail, string tempPassword)
        {
            var subject = "Password Recovery Email";
            var body = $"Your account password has been resetted. You can use temporary password to sign in and change password.{Environment.NewLine}Temporary password: {tempPassword}.";

            await SendEmail(recipientEmail, subject, body);
        }

        private async Task SendEmail(string recipientEmail, string subject, string body)
        {
            var mail = new MailMessage
            {
                From = new MailAddress(_senderEmail),
                Subject = subject,
                Body = body
            };

            mail.To.Add(new MailAddress(recipientEmail));

            var smtpClient = new SmtpClient("smtp.gmail.com", 587)
            {
                EnableSsl = true,
                UseDefaultCredentials = false,
                Credentials = new NetworkCredential(_senderEmail, _senderPassword)
            };

            try
            {
                await smtpClient.SendMailAsync(mail);
                Console.WriteLine("Email sent successfully.");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error sending email: " + ex.Message);
            }
        }
    }
}
