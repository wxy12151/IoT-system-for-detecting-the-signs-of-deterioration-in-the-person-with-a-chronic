import plotly.express as px
import pandas as pd

df1 = pd.read_csv("newData.csv")
order = []
b = []
for idx, data in df1.iterrows():
    order.append(data[0])

for idx, data in df1.iterrows():
    startdate = "1970-01-01 " + data[2]
    finishdate = "1970-01-01 " + data[3]
    a = {"Task": data[0], "Start": startdate, "Finish": finishdate, "Resource": data[1]}
    b.append(a)

b.append({"Task": "02/02/2009", "Start": "1970-01-01 7:15:17", "Finish": "1970-01-01 7:21:04", "Resource": "R1_Bed_to_Toilet"})
df = pd.DataFrame(b)
fig = px.timeline(df, x_start="Start", x_end="Finish", y="Task", color="Resource", title="Activity Occurrence",
                  opacity=0.9, )
fig.update_xaxes(
    tickformat="%H\n%M",
    tickformatstops=[
        dict(dtickrange=[3600000, 86400000], value="%H:%M")]  # range is 1 hour to 24 hours
)

fig.update_yaxes(categoryorder='array', categoryarray=order)
fig.update_yaxes(autorange="reversed")
fig.show()
fig.write_html("Activity_Chart.html")
