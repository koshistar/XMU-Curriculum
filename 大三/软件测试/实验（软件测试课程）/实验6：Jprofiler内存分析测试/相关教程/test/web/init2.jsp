<%--
  Created by IntelliJ IDEA.
  User: Administrator
  Date: 2020/4/3
  Time: 11:36
  To change this template use File | Settings | File Templates.
--%>
<%@ page language="java" import="cn.test.*" pageEncoding="ISO-8859-1"%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">

<html>

<head>

    <title>init</title>

</head>

<body><%

    for(int i=0;i<10000;i++){

        TestBean b = new TestBean();

        TestMain.list.add(b);

    }

%>

SIZE:<%=TestMain.list.size()%><br/>

counter:<%=TestMain.counter++%>

</body>

</html>
