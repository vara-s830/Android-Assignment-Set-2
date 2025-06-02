@Dao
public interface WeatherDao {
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    void insert(WeatherData weatherData);

    @Query("SELECT * FROM weather_data WHERE timestamp BETWEEN :start AND :end ORDER BY timestamp ASC")
    LiveData<List<WeatherData>> getWeatherDataBetween(long start, long end);

    @Query("SELECT * FROM weather_data ORDER BY timestamp DESC LIMIT 1")
    LiveData<WeatherData> getLatestWeather();
}
