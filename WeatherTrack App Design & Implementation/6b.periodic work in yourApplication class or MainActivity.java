PeriodicWorkRequest weatherSyncRequest =
    new PeriodicWorkRequest.Builder(WeatherSyncWorker.class, 6, TimeUnit.HOURS)
        .build();

WorkManager.getInstance(context).enqueueUniquePeriodicWork(
    "WeatherSync",
    ExistingPeriodicWorkPolicy.KEEP,
    weatherSyncRequest
);
