@Entity(tableName = "weather_data")
public class WeatherData {
    @PrimaryKey(autoGenerate = true)
    public int id;

    public long timestamp; // Unix time in millis

    public double temperature; // in Celsius
    public int humidity; // percentage
    public String condition; // e.g., "Sunny", "Cloudy"
}
