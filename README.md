软件功能描述

使用VC或QT语言编写

1. 设定操作：
(1)  串口设定（默认：波特率115200（9600~115200），无奇偶校验，一个停止位。可选），默认每次轮询时间间隔20（秒：5S~655535S），接收超时时间设定（单位：毫秒）,默认130毫秒，平台名设定（可输入）。
(2)  设备设置：

在平台下，设定井号名称（可输入，中英文），井深度(0~65535m)，该口井使用的通信板的板号，已经占用的不能再选用，板号默认递增，可选，也可以输入，板号占一个字节(01,02,03,04,05~255)。可快速删除井，同时该井下的所有层也删除。

在井号参数设定完成后，可以在该井下进行层数的设定

在井号下增加层：设定层名如第一层AA01，设定该层对应的序列号，离井口的深度（0到井深度，不能大于井深度，也不能重复，单位m），层目标流量值（单位m³/d），最大开度值（单位毫米），默认流量调整间隔(0~65535)。以此类推设定第二层AA02，第三层AA03，\*\*\*\*\*\*\*根据要求设置层数，最大为8层。可快速删除层，和修改其参数。

设置完第一口井，可以用同样方法设定第二口井,层名为AB01,AB02,AB03,\*\*\*\*\*\*\*\*\*\*

直到设定完规定井口数为止。

层数最大为8层。井数最大为3口井。

对设定后的以上配置可以进行保存，打开，打印。保存为TXT格式。保存内容包括：

串口设定参数，平台名，井名，通信板的板号，井深，层名，层序列号，层深，层目标流量值（单位m³/d），流量默认调整间隔，

1. 操作界面：
1  对每层可以进行如下操作：根据正确的序列号设定编号（编号和层名对应，如层名AA01对应编号01，AA02对应02，AB01对应编号01~~~~~~~~），询问流量设定值，询问工况，开度设定（与最大开度值的百分比：0~100%），询问当前开度值，流量值设定（单位m³/d），电机复位（应答成功后，需等待20S再进行该层的其他操作），调整间隔设定和询问，进入闭环模式，退出闭环模式，读取高压（V）、电流(A)操作，禁止询问工况使能和工况询问使能开启，进入闭环模式(对应层打个圆圈)，退出闭环模式（对应层打一斜杠）等。该层所有信息读取（包括显示序列号，编号，流量设定值，压力1，压力2，采样间隔，电机开度，温度，流量，目标流量，高压，高压电流，层状态故障显示（有故障，对应故障名显示红色，同时该层也显示灰色色）， 如改层通信不上则显示红色。
1  对所有层井下工况询问循环模式设定，间隔设定，进入工况询问模式。
1. 监控界面
1  对所有层的数据进行文本监控，实时更新。内容：井名，层名，压力1，压力2，设定流量，实时流量，温度，开度，高压，高压电流，故障状态信息显示，通信成功率。是否记录数据可以在这里选择（全局选择，部分选择），流量如超出设定流量值20%，显示红色，符合要求正常显示。通信失败，全显示红色。
1  可选择或不选择工况数据保存。每层存一个文件（文件名为：井名+层名+开始时间）：内容包括：时间，井名，层名，压力1，压力2，设定流量，流量，温度，开度，高压，高压电流，状态信息。
1  图表实时显示，可以选择每层数据，压力1，压力2，流量，温度，开度图表显示出来，只显示最近200次询问的数据。
1  实时监控每口井井口控制箱体箱体的高压电压和电流，可以根据需要关掉控制箱体对应的电压或开启。
1. 历史数据回放
1  选择保存的数据文件，图表显示。





