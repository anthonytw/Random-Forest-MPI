#ifndef __Node_h__
#define __Node_h__

#include <vector>
#include <string>
#include <sstream>

#include <ostream>
#include <istream>
#include <iostream>

/**
 * Node interface.
 */
class Node
{
  public:
    /**
     * A set of nodes.
     */
    typedef std::vector<Node*> NodeSet;

    /**
     * Node type indicator.
     */
    enum Type
    {
      InvalidType,
      SplitType,
      LeafType,
    };

  public:
    /**
     * Destructor.
     */
    virtual ~Node( void )
    {
      //
    }

    /**
     * Determine the action that led to the node.
     * @return The action that led to the node.
     */
    const std::string & get_action( void ) const
    {
      return action;
    }

    /**
     * Determine node type.
     * @return Node type.
     */
    const Type & get_type( void ) const
    {
      return type;
    }

    /**
     * Determine classification at this node.
     * @return The classification.
     */
    virtual bool get_classification( void ) const = 0;

    /**
     * Determine if the node is a leaf.
     * @return True if the node is a leaf, false otherwise.
     */
    virtual bool is_leaf( void ) const = 0;

    /**
     * Add a child to the node.
     * @param node The child node to add.
     */
    virtual void add_child( Node * const node ) = 0;

    /**
     * Get the children.
     * @return A list of all the node children.
     */
    virtual NodeSet get_children( void ) = 0;

    /**
     * Draw the node.
     * @return the GraphViz representation of the node.
     */
    virtual std::string draw( void ) const = 0;

    /**
     * Serialize the node and its children.
     * @param stream The output stream to serialize to.
     * @return The output stream.
     */
    static std::ostream & serialize( std::ostream & stream, const Node & node );

    /**
     * Deserialize the node and its children.
     * @param stream The input stream to deserialize from.
     * @return Pointer to the node and its children.
     */
    static Node * deserialize( std::istream & stream );

  protected:
    /**
     * Constructor.
     * @param action The action that led to the node.
     */
    Node( const std::string action, const Type type ) :
      type(type),
      action(action)
    {
      //
    }

  protected:
    const Type type;          ///< The node type.

  private:
    const std::string action; ///< The action that led to the node.
};

/**
 * Defines a leaf (decision) node.
 */
class LeafNode : public Node
{
  public:
    /**
     * Constructor.
     * @param action The action that led to the node.
     * @param classification The classification decision of the node.
     */
    LeafNode(
      const std::string action,
      const bool classification ) :
        Node(action, LeafType),
        classification(classification)
    {
      //
    }

    /**
     * Determine the classification.
     * @return The classification.
     */
    virtual bool get_classification( void ) const
    {
      return classification;
    }

    /**
     * Determine if the node is a leaf or not.
     * @return True, since this is a leaf node.
     */
    virtual bool is_leaf( void ) const
    {
      return true;
    }

    /**
     * Add a child to the node.
     * @param node The child node to add.
     */
    virtual void add_child( Node * const node )
    {
      // Do nothing.
    }

    /**
     * Get the children.
     * @return A list of all the node children.
     */
    virtual NodeSet get_children( void )
    {
      NodeSet set;
      return set;
    }

    /**
     * Draw the node.
     * @return the GraphViz representation of the node.
     */
    virtual std::string draw( void ) const
    {
      std::string draw_string =
        "shape=ellipse,label=\"(" + get_action() +
        ")\\n" + (classification ? "1" : "0") +"\"";
      return draw_string;
    }

    /**
     * Serialize the node.
     * @param stream The output stream to serialize to.
     * @param node The node.
     * @return The output stream.
     */
    static std::ostream & serialize_node( std::ostream & stream, const LeafNode & node )
    {
      std::size_t size = node.get_action().size();
      stream.write((char*)&size, sizeof(size));
      stream.write(node.get_action().c_str(), size);
      stream.write((char*)&node.classification, sizeof(node.classification));
      return stream;
    }

    /**
     * Deserialize the node.
     * @param stream The input stream to deserialize from.
     * @param node The node pointer to update.
     * @return The input stream.
     */
    static std::istream & deserialize_node( std::istream & stream, Node * &node )
    {
      // Read in action.
      std::size_t size = 0;
      stream.read((char*)&size, sizeof(size));
      std::string action("[*null*]");
      if ( size > 0 )
      {
        char action_buffer[size+1];
        stream.read(action_buffer, size);
        action_buffer[size] = 0;
        action = std::string(action_buffer);
      }

      // Read classification.
      bool classification = false;
      stream.read((char*)&classification, sizeof(classification));

      // Construct node.
      node = new LeafNode(action, classification);

      // Done.
      return stream;
    }

  private:
    const bool classification;  ///< Node classification.
};

/**
 * Defines a split node.
 */
class SplitNode : public Node
{
  public:
    /**
     * Constructor.
     * @param action The action the led to the node.
     * @param column The split column.
     * @param threshold The split threshold.
     */
    SplitNode(
      const std::string action,
      const unsigned int column,
      const double threshold ) :
        Node(action, SplitType),
        column(column),
        threshold(threshold)
    {
      //
    }

    /**
     * Destructor.
     * @note The destructor will delete all of the children.
     */
    virtual ~SplitNode( void )
    {
      if ( children.size() > 0 )
      {
        for (
          NodeSet::iterator iter = children.begin();
          iter != children.end(); ++iter )
        {
          delete *iter;
        }
      }
    }

    /**
     * Determine the classification.
     * @return The classification.
     * @note This function does not return a useful value.
     */
    virtual bool get_classification( void ) const
    {
      return false;
    }

    /**
     * Determine if the node is a leaf or not.
     * @return False, since this is not a leaf node.
     */
    virtual bool is_leaf( void ) const
    {
      return false;
    }

    /**
     * Add a child to the node.
     * @param node The child node to add.
     */
    virtual void add_child( Node * const node )
    {
      // Add child.
      children.push_back( node );
    }

    /**
     * Get the children.
     * @return A list of all the node children.
     */
    virtual NodeSet get_children( void )
    {
      return children;
    }

    /**
     * Draw the node.
     * @return the GraphViz representation of the node.
     */
    virtual std::string draw( void ) const
    {
      std::stringstream ss;
      ss << column;
      std::string draw_string =
        "shape=box,label=\"(" + get_action() +
        ")\\n" + ss.str() +"\"";
      return draw_string;
    }

    /**
     * Get the split column.
     * @return The split column.
     */
    unsigned int get_column( void ) const
    {
      return column;
    }

    /**
     * Get the split threshold.
     * @return The split threshold.
     */
    double get_threshold( void ) const
    {
      return threshold;
    }

    /**
     * Serialize the node.
     * @param stream The output stream to serialize to.
     * @return The output stream.
     */
    static std::ostream & serialize_node( std::ostream & stream, const SplitNode & node )
    {
      std::size_t size = node.get_action().size();
      stream.write((char*)&size, sizeof(size));
      stream.write(node.get_action().c_str(), size);
      stream.write((char*)&node.column, sizeof(node.column));
      stream.write((char*)&node.threshold, sizeof(node.threshold));

      // Children?
      size = node.children.size();
      stream.write((char*)&size, sizeof(size));
      if ( size > 0 )
      {
        for (
          NodeSet::const_iterator iter = node.children.begin();
          iter != node.children.end(); ++iter )
        {
          Node::serialize(stream, **iter);
        }
      }

      // Finished.
      return stream;
    }

    /**
     * Deserialize the node.
     * @param stream The input stream to deserialize from.
     * @param node The node pointer to update.
     * @return The input stream.
     */
    static std::istream & deserialize_node( std::istream & stream, Node * &node )
    {
      // Read in action.
      std::size_t size = 0;
      stream.read((char*)&size, sizeof(size));
      std::string action("[*null*]");
      if ( size > 0 )
      {
        char action_buffer[size+1];
        stream.read(action_buffer, size);
        action_buffer[size] = 0;
        action = std::string(action_buffer);
      }

      // Read column and threshold.
      unsigned int column = 0;
      double threshold = 0.0;
      stream.read((char*)&column, sizeof(column));
      stream.read((char*)&threshold, sizeof(threshold));

      // Construct node.
      node = new SplitNode(action, column, threshold);

      // Construct children, if any.
      std::size_t children_count = 0;
      stream.read((char*)&children_count, sizeof(children_count));
      if ( children_count > 0 )
      {
        for ( unsigned int c = 0; c < children_count; ++c )
        {
          Node * child = Node::deserialize(stream);
          node->add_child(child);
        }
      }

      // Done.
      return stream;
    }

  private:
    NodeSet children;             ///< The child nodes.
    const unsigned int column;    ///< The split column.
    const double threshold;       ///< The split threshold.
};

#endif
