public class WeatherRepository {

    private WeatherDao weatherDao;
    private WeatherApi weatherApi;
    private Executor executor;

    public WeatherRepository(Application application) {
        AppDatabase db = Room.databaseBuilder(application, AppDatabase.class, "weather_db").build();
        weatherDao = db.weatherDao();

        // Setup Retrofit with local mock JSON
        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl("file:///android_asset/") // or a mock server URL
                .addConverterFactory(GsonConverterFactory.create())
                .build();
        weatherApi = retrofit.create(WeatherApi.class);

        executor = Executors.newSingleThreadExecutor();
    }

    public LiveData<WeatherData> getLatestWeather() {
        return weatherDao.getLatestWeather();
    }

    public LiveData<List<WeatherData>> getWeatherForLast7Days() {
        long now = System.currentTimeMillis();
        long sevenDaysAgo = now - TimeUnit.DAYS.toMillis(7);
        return weatherDao.getWeatherDataBetween(sevenDaysAgo, now);
    }

    public void fetchAndSaveWeather(Callback<Void> callback) {
        weatherApi.getCurrentWeather().enqueue(new Callback<WeatherResponse>() {
            @Override
            public void onResponse(Call<WeatherResponse> call, Response<WeatherResponse> response) {
                if (response.isSuccessful() && response.body() != null) {
                    WeatherResponse res = response.body();
                    WeatherData data = new WeatherData();
                    data.timestamp = System.currentTimeMillis();
                    data.temperature = res.temperature;
                    data.humidity = res.humidity;
                    data.condition = res.condition;

                    executor.execute(() -> {
                        weatherDao.insert(data);
                        if (callback != null) callback.onResponse(null, null);
                    });
                } else {
                    if (callback != null) callback.onFailure(null, new Throwable("API Error"));
                }
            }

            @Override
            public void onFailure(Call<WeatherResponse> call, Throwable t) {
                if (callback != null) callback.onFailure(null, t);
            }
        });
    }
}
