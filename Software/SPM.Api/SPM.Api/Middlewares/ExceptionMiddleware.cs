using System.Net;
using System.Text.Json;
using SPM.Api.Core.Exceptions;
using System.Security.Authentication;

namespace SPM.Api.Middlewares
{
    public class ExceptionMiddleware
    {
        private readonly RequestDelegate _next;
        private readonly ILogger<ExceptionMiddleware> _logger;

        public ExceptionMiddleware(RequestDelegate next, ILogger<ExceptionMiddleware> logger)
        {
            _logger = logger;
            _next = next;
        }

        public async Task InvokeAsync(HttpContext httpContext)
        {
            try
            {
                await _next(httpContext);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.ToString());
                await HandleException(httpContext, ex);
            }
        }

        private async Task HandleException(HttpContext context, Exception exception)
        {
            context.Response.ContentType = "application/json";

            switch (exception)
            {
                case NotFoundException:
                    context.Response.StatusCode = (int)HttpStatusCode.NotFound;
                    break;
                case ArgumentException:
                    context.Response.StatusCode = (int)HttpStatusCode.BadRequest;
                    break;
                case AuthenticationException:
                    context.Response.StatusCode = (int)HttpStatusCode.Unauthorized;
                    break;
                default:
                    context.Response.StatusCode = (int)HttpStatusCode.InternalServerError;
                    break;
            }

            var response = new
            {
                StatusCode = context.Response.StatusCode,
                Error = exception.Message
            };

            await context.Response.WriteAsync(JsonSerializer.Serialize(response).ToString());
        }
    }
}
