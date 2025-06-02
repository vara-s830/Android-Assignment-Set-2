public class WeatherSyncWorker extends Worker {

    private WeatherRepository repository;

    public WeatherSyncWorker(@NonNull Context context, @NonNull WorkerParameters params) {
        super(context, params);
        repository = new WeatherRepository((Application) context.getApplicationContext());
    }

    @NonNull
    @Override
    public Result doWork() {
        try {
            // Synchronous fetch & save (simplified)
            CountDownLatch latch = new CountDownLatch(1);
            final Result[] result = {Result.failure()};

            repository.fetchAndSaveWeather(new Callback<Void>() {
                @Override
                public void onResponse(Call<Void> call, Response<Void> response) {
                    result[0] = Result.success();
                    latch.countDown();
                }

                @Override
                public void onFailure(Call<Void> call, Throwable t) {
                    result[0] = Result.retry();
                    latch.countDown();
                }
            });

            latch.await(30, TimeUnit.SECONDS);
            return result[0];
        } catch (InterruptedException e) {
            return Result.retry();
        }
    }
}
