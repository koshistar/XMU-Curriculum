import org.w3c.dom.*;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;


public class Main {
    public static void main(String[] args) {
//        System.out.println("Hello, World!");
        try {
            //创建DocumentBuilderFactory
            DocumentBuilderFactory factory=DocumentBuilderFactory.newInstance();
            //创建DocumentBuilder
            DocumentBuilder builder=factory.newDocumentBuilder();
            //解析XML文件
            Document document=builder.parse("pom.xml");
            //规范化文档
            document.getDocumentElement().normalize();
            //获取根元素
            Element root=document.getDocumentElement();
            System.out.println("根元素："+root.getNodeName());
            //遍历子节点
            NodeList nodeList=root.getChildNodes();
            for(int i=0;i<nodeList.getLength();i++)
            {
                Node node=nodeList.item(i);
                //只处理元素节点
                if(node.getNodeType()==Node.ELEMENT_NODE)
                {
                    Element element=(Element) node;
                    System.out.println("节点名："+element.getNodeName());
                    //获取属性
                    if(element.hasAttributes())
                    {
                        NamedNodeMap attributes=element.getAttributes();
                        for(int j=0;j<attributes.getLength();j++)
                        {
                            Node attr=attributes.item(j);
                            System.out.println("属性："+attr.getNodeName()+" = "+attr.getNodeValue());
                        }
                        //获取文本内容
                        System.out.println("内容："+element.getTextContent());
                    }
                }
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}