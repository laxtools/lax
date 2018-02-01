# Game Grinder 

[Grinder][http://grinder.sourceforge.net/] is a great distributed testing tool to load test TCP, HTTP/S servers.  

Game grinder is a load testing agent based on flow of actions. A flow is a test scenario configured in json to control the flow of action executions. Controls are jump to another action,  parallel jump, a.k.a, pump to another action running the current action on triggers of current action defiend for each action including "fail", "success". 

The above simple concept works for most games including MMORPGs, strategic RPGs, SNGs, and so on. It is proven efficient for in-house testing of several games. 

## Action

It is an atom of test execution like a unit test, but to cooperate flow of actions with other actions it has triggers. On a trigger jump / pump / exit are decided. Each action logs its execution statistics and those statistics can be analyzed later on. 

## Flow

Flow is a group of actions with names in its flow. Jump chooses name or next to jump or pump. 

Pump is important since some action needs to monitor server response even after the action triggers and jump to another action. 

The above simple scheme enables most of test scenarios. 

## Bot 

Bot is a virtual user simulating a real user in a game. 

## Runner 

Runner is a thread which runs Bots running Flow(s). 

## Reporter 

Reporter receives messages from Bots, Flow and Actions and handles them. CsvReporter is a typical one, logging message content into a CSV file. 

## Controller 

To run distributed tests, controller is required. Controller controls Agents to run Bots. Controller usually have monitor and report functionality with control. 

Grinder is one of the controllers to run distributed tests. There is no direct support to run C++ scripts. A controller with a simple user interface will be created together with Agent. 

## Agent 

Agent starts Runners to run Bots with Flows. Agents communicate with a controller to report, to get commands. 



 









