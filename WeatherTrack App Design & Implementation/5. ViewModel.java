public class WeatherViewModel extends AndroidViewModel {

    private WeatherRepository repository;
    private LiveData<WeatherData> latestWeather;
    private LiveData<List<WeatherData>> last7DaysWeather;

    private MutableLiveData<String> errorMessage = new MutableLiveData<>();

    public WeatherViewModel(@NonNull Application application) {
        super(application);
        repository = new WeatherRepository(application);
        latestWeather = repository.getLatestWeather();
        last7DaysWeather = repository.getWeatherForLast7Days();
    }

    public LiveData<WeatherData> getLatestWeather() {
        return latestWeather;
    }

    public LiveData<List<WeatherData>> getLast7DaysWeather() {
        return last7DaysWeather;
    }

    public LiveData<String> getErrorMessage() {
        return errorMessage;
    }

    public void refreshWeather() {
        repository.fetchAndSaveWeather(new Callback<Void>() {
            @Override
            public void onResponse(Call<Void> call, Response<Void> response) {
                // Success, clear error
                errorMessage.postValue(null);
            }

            @Override
            public void onFailure(Call<Void> call, Throwable t) {
                errorMessage.postValue(t.getMessage());
            }
        });
    }
}
