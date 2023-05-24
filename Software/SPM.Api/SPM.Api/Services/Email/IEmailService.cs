namespace SPM.Api.Services.Email
{
    public interface IEmailService
    {
        Task SendRecoveryEmail(string recipientEmail, string tempPassword);
    }
}
