#See https://aka.ms/containerfastmode to understand how Visual Studio uses this Dockerfile to build your images for faster debugging.

FROM mcr.microsoft.com/dotnet/runtime:6.0 AS base
WORKDIR /app

FROM mcr.microsoft.com/dotnet/sdk:6.0 AS build
WORKDIR /src
COPY ["MQTT.Broker/MQTT.Broker.csproj", "MQTT.Broker/"]
RUN dotnet restore "MQTT.Broker/MQTT.Broker.csproj"
COPY . .
WORKDIR "/src/MQTT.Broker"
RUN dotnet build "MQTT.Broker.csproj" -c Release -o /app/build --force

FROM build AS publish
RUN dotnet publish "MQTT.Broker.csproj" -c Release -o /app/publish /p:UseAppHost=false

FROM base AS final
WORKDIR /app
COPY --from=publish /app/publish .
ENTRYPOINT ["dotnet", "MQTT.Broker.dll"]
