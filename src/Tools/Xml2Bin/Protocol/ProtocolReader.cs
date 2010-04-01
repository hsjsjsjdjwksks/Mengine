using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;

namespace ProtocolReader
{
    public class Protocol
    {

        Dictionary<string, string> typeDict = new Dictionary<string, string>();
        Dictionary<string, int> nodeIdDict = new Dictionary<string, int>();
        Dictionary<string, int> attrIdDict = new Dictionary<string, int>();
        Dictionary<string, string> headerStructName = new Dictionary<string, string>();

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_fileName"></param>
        public void updateFromXml(string _fileName)
        {
            Console.WriteLine("Updating protocol " + _fileName);

            XmlDocument protocolXml = new XmlDocument();
            protocolXml.Load(_fileName);
            XmlNodeList nodeList = protocolXml.GetElementsByTagName("Node");

            int nodeID = 0;
            int attrID = 0;
            foreach (XmlNode node in nodeList)
            {
                string nodeType = node.Attributes.GetNamedItem("NodeType").Value;

                if (!nodeIdDict.ContainsKey(nodeType))
                {
                    nodeIdDict.Add(nodeType, nodeID);
                    nodeID++;
                }

                if (node.Attributes.Count == 1) continue;


                string attrName = node.Attributes.GetNamedItem("AttrName").Value;

                string key = nodeType + "." + attrName;

                try
                {
                    headerStructName.Add(key, nodeType + "_" + attrName);
                }
                catch (Exception e)
                {
                    System.Console.WriteLine(e.Message);
                    System.Console.WriteLine(key);
                    continue;
                }

                attrIdDict.Add(key, attrID);


                string attrType = node.Attributes.GetNamedItem("AttrType").Value;
                typeDict.Add(key, attrType);

                attrID++;



            }

            Console.WriteLine("Protocol updating complete");
        }
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_fileName"></param>
        public Protocol(string _fileName)
        {
            updateFromXml(_fileName);
        }
        /// <summary>
        /// 
        /// </summary>
        public Dictionary<string, string> TypeDict
        {
            get
            {
                return typeDict;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public Dictionary<string, int> NodeIdDict
        {
            get
            {
                return nodeIdDict;
            }
        }

        /// <summary>
        /// 
        /// </summary>       
        public Dictionary<string, int> AttrIdDict
        {
            get
            {
                return attrIdDict;
            }
        }

        public Dictionary<string, string> HeaderStructName
        {
            get
            {
                return headerStructName;
            }
        }
    }
}
