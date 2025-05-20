var builder = DistributedApplication.CreateBuilder(args);

var apiService = builder.AddProject<Projects.CS_LAB06_ApiService>("apiservice");

builder.AddProject<Projects.CS_LAB06_Web>("webfrontend")
    .WithExternalHttpEndpoints()
    .WithReference(apiService);

builder.Build().Run();
